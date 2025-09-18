from datetime import date
import os
import shutil

project = "Exit Guardian"
author = "Nuno Peixoto"
copyright = f"{date.today().year}, {author}"
version = ""
release = ""

extensions = [
    "myst_parser",            # Markdown
    "sphinx_needs",           # Requirements/traceability
    "sphinxcontrib.plantuml", # PlantUML (local)
]

myst_enable_extensions = ["deflist", "colon_fence", "substitution", "attrs_block"]

templates_path = ["_templates"]
exclude_patterns = ["_build"]
html_theme = "furo"
html_static_path = ["_static"]

# ---------------- PlantUML (local) ----------------
# Path to your PlantUML .jar (recommend keeping it inside the repo)
PLANTUML_JAR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "tools", "plantuml.jar"))

# Verify Graphviz 'dot' is present; PlantUML relies on it
if not shutil.which("dot"):
    print("WARNING: Graphviz 'dot' not found on PATH. Install graphviz to render diagrams.")

# Use the .jar directly
plantuml = f"java -jar {PLANTUML_JAR}"
plantuml_output_format = "svg"   # crisp & searchable
plantuml_latex_output_format = "pdf"

# ------------- sphinx-needs config ---------------
needs_types = [
    {"directive": "req",  "title": "Requirement", "prefix": ""},
    {"directive": "arch", "title": "Architecture", "prefix": "ARCH-"},
    {"directive": "test", "title": "Test Case",    "prefix": "TEST-"},
]

# Note: 'status' is built-in; don't redeclare it in needs_extra_options
needs_extra_options = ["priority", "component"]

needs_statuses = [
    {"name": "draft",       "style": "gray",  "description": "Draft"},
    {"name": "approved",    "style": "green", "description": "Approved"},
    {"name": "implemented", "style": "blue",  "description": "Implemented"},
    {"name": "verified",    "style": "orange","description": "Verified by tests"},
    {"name": "deprecated",  "style": "red",   "description": "Deprecated"},
]

latex_engine = "xelatex"
latex_elements = {
    "papersize": "a4paper",
    "pointsize": "11pt",
}
