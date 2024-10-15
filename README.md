# Opus Project Base

## How to setup a new project

Bash
```sh
project_name='sample-project' \
&& git clone --depth=1 --branch=main https://github.com/a98c14/opus-project-base.git ./$project_name \
&& cd $project_name \
&& rm -rf ./.git \
&& unset project_name \
&& git init;
```

Powershell
```ps1
$project_name=Read-Host "Please enter the name of your project."; `
git clone --depth=1 --branch=main https://github.com/a98c14/opus-project-base.git ./$project_name; `
cd $project_name; `
Remove-Item ./.git -Recurse -Force; `
Clear-Variable project_name; `
git init;
cd src; `
git submodule add https://github.com/a98c14/opus.git; `
cd ..; `
New-Item -ItemType Directory -Force -Path ".\dist"; `
Copy-Item -Path ".\dependencies\*" -Destination ".\dist"; `
code .;
```
