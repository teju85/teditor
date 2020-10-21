#!/bin/bash
# Thanks to: https://www.innoq.com/en/blog/github-actions-automation/

echo "----- All env-vars -----"
env
echo "----- All env-vars -----"
echo

set -eu

repo_uri="https://x-access-token:${DEPLOY_TOKEN}@github.com/${GITHUB_REPOSITORY}.git"
remote_name="origin"
main_branch="master"
target_branch="gh-pages"
build_dir="dist"

cd "$GITHUB_WORKSPACE"

git config user.name "$GITHUB_ACTOR"
git config user.email "${GITHUB_ACTOR}@bots.github.com"

make doc
git checkout -t "$remote_name/$target_branch"
rm -rf *.html *.js *.css *.png search/
cp -r bin/html/html/* .

git add .
git commit -m "updated GitHub Pages"
if [ $? -ne 0 ]; then
    echo "nothing to commit"
    exit 0
fi

git remote set-url "$remote_name" "$repo_uri" # includes access token
git push --force-with-lease "$remote_name" "$target_branch"
