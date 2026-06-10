#!/bin/bash

cd 2003 || exit
while read f ;
	
	do echo $f; ../strip_non_ascii.out  "$f" > "$f"_cleaned ;
done < <(find . -type f -regex "./03[0-9]+.tex")
	
while read f ;
	do 
		original=$(sha256sum "$f"|cut -d ' ' -f1);
		cleaned=$(sha256sum "$f"_cleaned|cut -d ' ' -f1);
	if [[ $original == $cleaned ]] ; 
		then 
			rm "$f"_cleaned ;
			mv "$f" "$f"
	else 
		mv "$f" ../2003_errors;
		mv "$f"_cleaned "$f"_cleaned.tex
	fi ;
done < <(find . -type f -regex "./03[0-9]+.tex")

