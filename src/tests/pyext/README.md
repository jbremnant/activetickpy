# Extending Python

Excellent guide here: https://docs.python.org/2/extending/extending.html

## Testing

```
$ python setup.py build
$ python setup.py install --prefix ./install
```

```
In [1]: import sys

In [2]: sys.path.append('install/lib/python2.7/site-packages/')

In [3]: import spam

In [4]: spam.
spam.c       spam.error   spam.system

In [4]: spam.error
Out[4]: spam.error

In [5]: spam.system
Out[5]: <function spam.system>

In [6]: spam.system('date')
Sun Aug 17 21:40:43 EDT 2014
cmd: date
Out[6]: 0
```
