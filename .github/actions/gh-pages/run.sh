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

echo "Setting up user info..."
git config user.name "$GITHUB_ACTOR"
git config user.email "${GITHUB_ACTOR}@bots.github.com"

echo "Building docs..."
make doc
echo "Switching to gh-pages..."
git checkout "$target_branch"
echo "Removing old documentation..."
rm -rf *.html *.js *.css *.png search/
echo "Copying the new documentation..."
cp -r bin/GNU_Linux/html/* .
echo "Committing..."
git add .
git commit -m "updated GitHub Pages"
if [ $? -ne 0 ]; then
    echo "nothing to commit"
    exit 0
fi
echo "Pushing..."
git remote set-url "$remote_name" "$repo_uri" # includes access token
git push --force-with-lease "$remote_name" "$target_branch"
