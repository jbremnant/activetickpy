# ActiveTick Python Library

This is BETA! While it serves the purpose of pulling tick data, it's by no means production software.
The following library works the plumbing from activetick C++ api into python.
Only tested on linux.

* The communication between activetick and python is facilitated using callbacks.
* Activetick library has more features, this merely exposes the small set of features.
* More documentation of Activetick's api is
  [here](http://www.activetick.com/activetick/contents/PersonalServicesDataAPIDownload.aspx)


## To Install and Test

```
$ git clone git@github.com:jbremnant/activetickpy.git
$ cd activetickpy/activetick

# This will only create the installation in the current directory
$ ./compile.sh  

# ensures you can find the .so file
$ . ./env.sh  

$ python at_test.py <api_key> <userid> <passwd>
```

## Shared Lib

Python needs to be able to see `libActiveTickServerAPI.so` file. As such, the following
might be necessary in your shell env:

```
export LD_LIBRARY_PATH=/path/to/lib/
```

## Initiating Connection

```{python}
mport pandas as pd
import sys
sys.path.append('install/lib/python2.7/site-packages/')
import _activetick as at

print(at.version())

a = 0;

def mylogincb(i, msg):
  global a
  a += 1
  print(msg)

at.set_login_callback(myfunc)
at.connect(<your_api_key>,<server>,<port>,<userid>,<password>)

print(at.did_login())
loopcount = 0
while not at.did_login():
  loopcount += 1

print("global increment: %d, loop count: %d" %(a,loopcount))
```

## Retrieving Tick Data

```{python}
class mytick(object):
  def __init__(self):
    self.data = {}

  def cb(self, sym, l):
    print "Python mytickcb received %s data with len: %d" % (sym,len(l))
    df = pd.DataFrame(l)
    df['symbol'] = sym
    if sym in self.data.keys():
      self.data[sym] = pd.concat([self.data[sym], df])
    else:
      self.data[sym] = df

  def to_store(self, fname):
    store = pd.HDFStore(fname)
    for i in self.data.keys():
      store.put(i, self.data[i], format='table')
    store.close()


o = mytick()

at.set_tickhist_callback(o.cb)
at.make_tick_request("RHT", "20130522", "20130522")
at.make_tick_request("BAC", "20130522", "20130522")

while at.num_pending_requests() > 0:
  pass

# accumulated pandas dataframe
print o.data
o.to_store('./test.h5')
```
