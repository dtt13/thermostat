# We need to get the stored user name and password.
# Our inputs to this script are the user name
# and password entered by the user in the browser.
# The stored user name and password should never
# be accessed outside of this method, in order to
# ensure security.

# Currently, this file is meant to be imported
# into app.py for repeated use.

from hashlib import sha256

# Module to compare the passwords
def login_cmp(usn, pwd):
    try:
        f = open(usn, 'r')
        hashword = f.readline()
        f.close()
    except IOError:
        return 0
    except:
        return 0
    pwdhash = sha256(pwd).hexdigest()
    if pwdhash==hashword:
        return 1
    return 0