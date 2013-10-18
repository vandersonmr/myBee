#!/usr/bin/env python
# -*- coding: cp860 -*-

from distutils.core import setup, Extension

repaModule = Extension('repa',
                    include_dirs = ['.', '..'],
                    libraries = ['rt', 'pthread', 'm'],
                    sources = ['lib/python_lib-repa.c', 
                               'lib/lib-repa.c',  
                               'util/util.c', 
                               'util/linkedlist.c',
							   'util/hashmap.c'])

setup (name = 'RepaModule',
       version = '0.2.4',
       description = 'This is a REP API package for Python',
       author = 'Heberte Fernandes de Moraes',
       author_email = 'heberte@ufjr.br',
       url = 'http://www.lcp.coppe.ufrj.br/radnet',
       ext_modules = [repaModule])
