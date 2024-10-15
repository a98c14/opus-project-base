# Opus Project Base

## How to setup a new project

**Bash**
```sh
project_name='sample-project' \
&& git clone --depth=1 --branch=main https://github.com/a98c14/opus-project-base.git ./$project_name \
&& cd $project_name \
&& rm -rf ./.git \
&& unset project_name \
&& git init;
```

**Powershell**
Run the following command where you want to create your project.
```ps1
Invoke-RestMethod -Uri https://gist.githubusercontent.com/a98c14/93b83508b8cc7f0c9e72d3ca8ff31ad8/raw/3747aaa7bfef1bec1884cb333d6bc6c5a6672830/opus_new_project.ps1 | Invoke-Expression
```

