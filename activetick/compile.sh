#!/bin/bash
python setup.py build --force
python setup.py install --prefix `pwd`/install
