#!/bin/bash

push(){
    git add .
    #echo "Enter commit message"
    #read commit_message
    git commit -m "$1"
    git push
    echo "build pushed"
}

push "$1"