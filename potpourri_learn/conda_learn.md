Pip freeze
Can freeze the requirements using pip freeze > requirements.txt
And then reload them using 

python -m venv newenv
source newenv/bin/activate
pip install -r requirements.txt


Pip install vs conda install:
Pip installs from python package index PyPI and conda from conda repos
Pip install numpy will download numpy in your environment, but if it depends on a c source library it may try to build it from source which can fail
Conda install numpy will download a binary with all the accompanying it, which can be slower but safer
(PIP FOLLOWS THE PYTHON INTERPRETER, CONDA MANAGES THE ENV AS A WHOLE)



## CONDA
Conda Tasks:

conda --help for general help and
conda [term, exe env] --help for detailed assistance on subcommands

Conda –version to get the current verison

Conda update conda

Conda create –name <myenv> creates a new environment in teh /envs/ directory with 

conda create -n myenv python=3.12

(conda env create vs conda create: env create requires a environment.yml file!)
conda env create -f environment.yml (the name is baked into the environment!!!)






conda activate myenv #activates myenv

conda deactivate

conda remove -n myenv --all

conda env list #lists all environments

conda list <package_name>

conda list -n myenv numpy

conda install numpy

Conda install numpy=3.1.2

conda search pandas



conda remove scipy

conda update matplotlib

conda list #lists all packages in an environment

CRUTIAL:

Conda search torchvision –info to see dependencies

conda install -c conda-forge torchvision to install from the specific channel

conda install python=3.12 --update-deps --force-reinstall
#this will make it update dependencies to whatever version is unavailable
#and will also force the re-install, even if you have it already

conda env create --name ai_mac --file environment.yml

conda env export > environment.yml #exports your environment to a .yml file

conda env export --no-builds > environment.yml //removes the build, keeping it OS-Independent

conda env export --from-history > environment.yml #only minimal list of what you’ve explicitly installed (better for sharing)

conda env update -f environment.yml --prune  #updates your environment to include the new files in teh environment.yml, with –prune meaning to remove all files which are not in the environment.yml to keep everyone up to date



conda env create -f environment.yml #creates env from a .yml file (name with -n myenv)



NOTE: when creating a terminal in vscode:
Inherited PATH from parent app
VS Code, tmux, or a login manager started while myenv was active; all new terminals inherit that PATH until you restart the parent process.



NOTES:
conda works with pip, but the files do not automaticallly appear in teh environment.yml file!
THAT BEING SAID, THEY ARE INCLUDED UNDER PIP AS A SUB-DEPENDENCY IN A NESTED BLOCK:
dependencies:
  - python=3.11
  - numpy=1.26


  - pip:
      - requests==2.31.0
      (see, its a sub-area)

