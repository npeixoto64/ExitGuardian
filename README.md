# ExitGuardian
A full product project that watches windows with wireless sensors and has a gateway for the main door with wireless sensors.
When the user opens the exit door to leave, it checks for open windows and doors.

# Documentation
## How to generate the documentation
$ python3 -m venv .venv
$ source .venv/bin/activate
$ pip install -r requirements.txt
$ make html

## Deactivate the python virtual environment
$ deactivate
$ rm -rf .venv
