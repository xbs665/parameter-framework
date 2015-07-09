# Copyright (c) 2014-2015, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import logging
import json
import os


class ConfigParser:

    """ This class define needed configuration environment information """

    def __init__(self, confFileName, testsDirectory, consoleLogger):

        # Parsing of Json test file
        with open(confFileName, "r") as testFile:
            self.__conf = json.load(testFile)

        # Preparing files and directory paths
        for key in ["CriterionFile",
                    "PfwConfFile"
                    "ScriptsFile",
                    "ActionGathererFile",
                    "ScenariosDirectory",
                    "LogFile",
                    "CoverageFile",
                    "CoverageDir",
                    "PfwDomainConfFile"]:
            self.__conf[key] = os.path.join(testsDirectory, self.__conf[key])

        self.__logger = logging.getLogger(__name__)
        self.__logger.addHandler(consoleLogger)

    def __getitem__(self, item):
        try:
            return self.__conf[item]
        except KeyError as e:
            self.__logger.error(
                "The item : {} is not in the configuration file".format(item))
            raise e
