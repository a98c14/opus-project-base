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
Invoke-RestMethod -Uri https://gist.githubusercontent.com/a98c14/93b83508b8cc7f0c9e72d3ca8ff31ad8/raw/c00371c55b26b35c9b1cc53638126a75a082d90b/opus_new_project.ps1 | Invoke-Expression
```

