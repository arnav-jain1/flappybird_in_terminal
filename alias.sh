#!/bin/bash

name="fb"

curr=$(pwd)
alias="alias $name='$curr/flappy_bird'"

if [ -n "$ZSH_VERSION" ]; then
    CONFIG_FILE="$HOME/.zshrc"
elif [ -n "$BASH_VERSION" ]; then
    CONFIG_FILE="$HOME/.bashrc"
else
    echo "Unsupported shell. Please add the alias manually to your shell configuration file."
    exit 1
fi

if grep -qF "$name" "$CONFIG_FILE"; then
    echo "Alias with name '$name' already exists in $CONFIG_FILE"
else
    echo "" >> "$CONFIG_FILE"
    echo "# Flappy Bird game alias" >> "$CONFIG_FILE"
    echo "$alias" >> "$CONFIG_FILE"
    echo "Alias '$name' added to $CONFIG_FILE"
fi

echo "To use the new alias, either run 'source $CONFIG_FILE' or restart your terminal."
