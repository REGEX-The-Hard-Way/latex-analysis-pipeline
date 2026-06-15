UPDATE authors
SET token = (
    WITH RECURSIVE strip AS (
        SELECT 
            a.token_id AS author_id,
            REPLACE(a.token, t.token, '') AS current,
            t.token_id AS done_child,
            1 AS step
        FROM authors a
        JOIN authors t ON t.parent_id = a.token_id
        WHERE a.type = 'author'
          AND a.token_id = authors.token_id
        
        UNION ALL
        
        SELECT 
            s.author_id,
            REPLACE(s.current, t.token, ''),
            t.token_id,
            s.step + 1
        FROM strip s
        JOIN authors t ON t.parent_id = s.author_id 
            AND t.token_id > s.done_child
            AND t.type != 'author'
    )
    SELECT current 
    FROM strip 
    ORDER BY step DESC 
    LIMIT 1
)
WHERE type = 'author'
  AND EXISTS (
      SELECT 1 FROM authors t 
      WHERE t.parent_id = authors.token_id 
        AND t.type != 'author'
  );
