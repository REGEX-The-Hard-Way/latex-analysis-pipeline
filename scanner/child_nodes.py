import sqlite3
import pandas as pd

conn = sqlite3.connect("/mnt/x/home/user/my_data.db")
cursor = conn.cursor()
df = pd.read_sql("select rowid, * from authors", conn)


def replace_leaf_sqlite_style(df, parent_type="author", leaf_types=None):
    """
    Recursively replace leaf nodes from parent tokens.

    Parameters:
    df: DataFrame with columns [token_id, parent_id, type, token]
    parent_type: Type to treat as parent (default: 'author')
    leaf_types: List of types to replace (default: all except parent_type)

    Returns:
    DataFrame with replaced parent tokens
    """
    result = df.copy()

    # If no leaf_types specified, use all types except parent_type
    if leaf_types is None:
        leaf_types = df[df["type"] != parent_type]["type"].unique().tolist()

    # Build parent-leaf relationships (only for specified leaf types)
    leaf_map = {}
    leaf_df = df[df["type"].isin(leaf_types)]

    for _, row in leaf_df.iterrows():
        parent_id = row["parent_id"]
        if pd.notna(parent_id) and parent_id:
            if parent_id not in leaf_map:
                leaf_map[parent_id] = []
            leaf_map[parent_id].append(
                {"token": row["token"], "token_id": row["token_id"]}
            )

    # Get leaf IDs mapping for recursion
    leaf_id_map = {}
    for _, row in leaf_df.iterrows():
        parent_id = row["parent_id"]
        if pd.notna(parent_id) and parent_id:
            if parent_id not in leaf_id_map:
                leaf_id_map[parent_id] = []
            leaf_id_map[parent_id].append(row["token_id"])

    # Recursively replace leaf from parents
    def replace_recursive(text, parent_id, visited=None):
        if visited is None:
            visited = set()

        if parent_id in visited or parent_id not in leaf_map:
            return text

        visited.add(parent_id)

        # Strip all direct leaf of specified types
        for leaf in leaf_map[parent_id]:
            text = text.replace(leaf["token"], "")

        # Recurse to grandleaf (also of specified types)
        if parent_id in leaf_id_map:
            for leaf_id in leaf_id_map[parent_id]:
                if leaf_id in leaf_map:
                    text = replace_recursive(text, leaf_id, visited)

        return text

    # Apply to all parents
    parents = result[result["type"] == parent_type]
    for idx in parents.index:
        parent_id = result.at[idx, "token_id"]
        if parent_id in leaf_map:
            cleaned = replace_recursive(result.at[idx, "token"], parent_id)
            result.at[idx, "token"] = cleaned

    return result


z = replace_leaf_sqlite_style(
    df, parent_type="author", leaf_types=["footnote", "email", "thanks"]
)

# Write modified rows back to the database
updated_count = 0
for idx in df.index:
    if df.at[idx, "type"] == "author" and df.at[idx, "token"] != z.at[idx, "token"]:
        rowid = int(z.at[idx, "rowid"])
        cursor.execute(
            "UPDATE authors SET token = ? WHERE rowid = ?",
            (z.at[idx, "token"], rowid),
        )
        updated_count += 1

conn.commit()
conn.close()
print(f"Updated {updated_count} author rows in the database.")
