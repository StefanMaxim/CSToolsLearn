# Repository Management
Once you start working in larger codebases, cannot just toss everything in a big directory with a bunch of random files
best layout (for python) is the src model.
this is good bc it prevent issues where your project's root folder can be treated as a python dependency
this way, all imports are relative to the source folder making it easier to handle:

exe:
suppose you have a file called my_module.py inside of my_project/ (the root dir). 
in another python file, to import the module, you would have to do:

```python
from my_project import my_module
```

but if you dont organize well, it can be confusing to python where to find it. Thus, solution is to create a folder called
src/ ,so python knows to look inside of src for actual source code

same for testing: when testing, want to ensure you are testing real code in your project, not somethign random:


Way it works:
this is your directory:

my_project/
├── src/
│   └── my_package/
│       └── my_module.py
├── tests/
│   └── test_my_module.py
├── setup.py
└── README.md


When python looks for files to import, it will look in your current directory and the standard libraries
when using the src/ layout, must tell python your main code is in the src/ folder.

2 ways to do this:
1: configure your python path to have source
2: add a __init__.py in your project, letting python know its part of your project


This lets you tell python that src is part of your projects
export PYTHONPATH=src
python -m my_package.my_module

then you can 
from my_package import my_module (which it will now know where to find)

**NOTE** pytest is smart enough to find the tests in /tests though




## Back to REPO Structure:

src/ is product code
configs/ is behavior and experiment settings
scripts/ is entrypoints and one-off runners
tests/ proves the code works
docs/ explains how the repo is organized and how to extend it

example full codebase:

repo/
├─ README.md
├─ pyproject.toml
├─ .gitignore
├─ .env.example
├─ Makefile                     # optional, but great for common commands
├─ CHANGELOG.md
├─ CONTRIBUTING.md
│
├─ configs/
│  ├─ base/
│  │  ├─ model.yaml
│  │  ├─ data.yaml
│  │  ├─ train.yaml
│  │  └─ eval.yaml
│  ├─ experiments/
│  │  ├─ sft_llama3_small.yaml
│  │  ├─ dpo_v2.yaml
│  │  └─ aug_ablation_01.yaml
│  └─ augmentation/
│     ├─ backtranslation.yaml
│     ├─ paraphrase.yaml
│     └─ filtering.yaml
│
├─ docs/
│  ├─ index.md
│  ├─ architecture.md
│  ├─ repo_map.md
│  ├─ training.md
│  ├─ augmentation.md
│  ├─ evaluation.md
│  └─ adding_new_feature.md
│
├─ scripts/
│  ├─ train.py
│  ├─ evaluate.py
│  ├─ prepare_dataset.py
│  ├─ augment_dataset.py
│  ├─ export_model.py
│  └─ smoke_test.py
│
├─ src/
│  └─ llm_finetune/
│     ├─ __init__.py
│     ├─ cli/
│     │  ├─ train.py
│     │  ├─ eval.py
│     │  └─ augment.py
│     ├─ data/
│     │  ├─ __init__.py
│     │  ├─ schemas.py
│     │  ├─ loaders.py
│     │  ├─ writers.py
│     │  ├─ splits.py
│     │  ├─ validation.py
│     │  └─ preprocessing.py
│     ├─ augmentation/
│     │  ├─ __init__.py
│     │  ├─ base.py
│     │  ├─ registry.py
│     │  ├─ paraphrase.py
│     │  ├─ backtranslation.py
│     │  ├─ self_instruct.py
│     │  └─ filters.py
│     ├─ training/
│     │  ├─ __init__.py
│     │  ├─ trainer.py
│     │  ├─ loss.py
│     │  ├─ callbacks.py
│     │  ├─ checkpointing.py
│     │  └─ distributed.py
│     ├─ models/
│     │  ├─ __init__.py
│     │  ├─ factory.py
│     │  ├─ tokenizer.py
│     │  ├─ peft.py
│     │  └─ loading.py
│     ├─ evaluation/
│     │  ├─ __init__.py
│     │  ├─ metrics.py
│     │  ├─ benchmarks.py
│     │  ├─ generation.py
│     │  └─ reports.py
│     ├─ pipelines/
│     │  ├─ sft_pipeline.py
│     │  ├─ dpo_pipeline.py
│     │  └─ augmentation_pipeline.py
│     ├─ utils/
│     │  ├─ logging.py
│     │  ├─ seed.py
│     │  ├─ io.py
│     │  └─ paths.py
│     └─ tracking/
│        ├─ __init__.py
│        ├─ experiment.py
│        └─ artifacts.py
│
├─ tests/
│  ├─ conftest.py
│  ├─ unit/
│  │  ├─ test_validation.py
│  │  ├─ test_augmentation.py
│  │  └─ test_metrics.py
│  ├─ integration/
│  │  ├─ test_train_smoke.py
│  │  └─ test_augment_pipeline.py
│  └─ fixtures/
│     └─ sample_dataset.jsonl
│
├─ data/                        # usually not fully committed
│  ├─ raw/
│  ├─ interim/
│  ├─ processed/
│  └─ samples/
│
├─ outputs/
│  ├─ runs/
│  ├─ checkpoints/
│  ├─ predictions/
│  └─ reports/
│
└─ notebooks/
   ├─ exploratory/
   └─ analysis/

The key rule is: every directory should answer one clear question.
For example:

data/ = how data is loaded, validated, cleaned
augmentation/ = how new examples are generated or filtered
training/ = how optimization happens
evaluation/ = how you score outputs
pipelines/ = how multiple steps are chained together
scripts/ = what humans run from the command line (note: scripts should be thin wrappers, real logic should live in src)

If it is reusable logic, put it in src/
If it is a config or experiment setting, put it in configs/
If it is documentation for humans, put it in docs/
If it is a command someone runs, put it in scripts/
If it checks behavior, put it in tests/
If it is generated output, put it in outputs/
If it is temporary exploration, put it in notebooks/





















**NOTES**
Notes on the structure:
what is the __init__.py files?

python treats normal directories as directories unless they have the __init__.py file, which makes it clear to python 
that is is a package. That means the folder can have modules which can be imported.

ALSO NOTE: inside teh __init__.py file, it contains the initialization of the package, ie code that it run when you import a package
Good for defining certain variables, importing submodules, or defining package level functionality

ALSO: it is used to control the namespace of the package, ie which subpackages can be invoked from it
```python
# Inside __init__.py
from .submodule import some_function
```

then in your code:
```python
import mypackage
mypackage.some_function()
```

my_package/
    __init__.py
    module1.py
    module2.py
    subpackage/
        __init__.py
        submodule.py

from my_package import module1
from my_package.subpackage import submodule

(BY DEFAULT, ONLY .PY FILES ARE ACCESSIBLE IN A PACKAGE!!)
(IN THE __init__, YOU CAN IMPORT SUBFUNCTIONS/CLASSES FROM SUBMODUELS AND MAKE THEM ACCESSIBLE IN THE PACKAGE ITSELF)
TO MAKE SUBPACKAGES AVAILABLE, YOU MUST:

```python
# my_package/__init__.py
from . import subpackage
``` 
to import the subpackage to the package, so it is accessible

