# SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# We import core and gxf to make sure they're available before other modules that rely on them
from . import core, gxf

as_tensor = core.Tensor.as_tensor
__all__ = ["as_tensor", "core", "gxf"]

# Other modules are exposed to the public API but will only be lazily loaded
_EXTRA_MODULES = [
    "conditions",
    "executors",
    "graphs",
    "logger",
    "operators",
    "resources",
]
__all__.extend(_EXTRA_MODULES)


# Autocomplete
def __dir__():
    return __all__


# Lazily load extra modules
def __getattr__(name):
    import importlib
    import sys

    if name in _EXTRA_MODULES:
        module_name = f"{__name__}.{name}"
        module = importlib.import_module(module_name)  # import
        sys.modules[module_name] = module  # cache
        return module
    else:
        raise AttributeError(f"module {__name__} has no attribute {name}")
