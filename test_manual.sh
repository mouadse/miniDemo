#!/bin/bash

# Set up environment
export EMPTY=""

# Test the command
echo '$EMPTY echo hi' | ../minishell > mini_output.txt 2>&1
echo '$EMPTY echo hi' | bash > bash_output.txt 2>&1

echo "Mini output:"
cat mini_output.txt | grep -v "Minishell" | grep -v "exit" | sed '/^$/d'
echo "Bash output:"  
cat bash_output.txt

rm -f mini_output.txt bash_output.txt