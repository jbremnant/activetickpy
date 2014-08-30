#!/usr/bin/env python

#
# In this current directory, make sure you run on your shell:
#   $ . ./env.sh
#

import pandas as pd
import sys
sys.path.append('install/lib/python2.7/site-packages/')


def run(apikey,userid,passwd):
  import _activetick as at

  print(at.version())

  a = 0;
  
  def myfunc(i, msg):
    global a
    a += 1
    print(msg)
  
  at.set_login_callback(myfunc)
  at.connect(apikey,"activetick1.activetick.com","5000",userid,passwd)
  
  print(at.did_login())
  loopcount = 0
  while not at.did_login():
    loopcount += 1
  
  
  print("global increment: %d, loop count: %d" %(a,loopcount))
  
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
    def to_csv(self, fdir):
        for i in self.data.keys():
            csvfile = "%s/%s.csv" % (fdir, i)
            print "writing to %s" % csvfile
            self.data[i].to_csv(csvfile)
  
  o = mytick()
  
  at.set_tickhist_callback(o.cb)
  at.make_tick_request("RHT", "20130522", "20130522")
  # at.make_tick_request("BAC", "20130522", "20130522")
  
  while at.num_pending_requests() > 0:
    pass
  
  # accumulated pandas dataframe
  print o.data
  # o.to_store('/tmp/test.h5')
  o.to_csv('/tmp/')
  print ("stored data to: /tmp/test.h5")
  return True


if __name__ == "__main__":
  if len(sys.argv) < 4:
    print "usage: %s <apikey> <user> <passwd>" % sys.argv[0]
    sys.exit(1)

  args = sys.argv[1:4]
  sys.exit(run(*args))
