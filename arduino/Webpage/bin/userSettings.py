import hashlib

def checkUserInfo(user_in, pass_in):
    f = open("bin/userinfo.txt","r")
    username = f.readline().rstrip('\n')
    password = f.readline().rstrip('\n')
    f.close()
    ph = hashlib.sha256(pass_in)
    pwd_hash = ph.hexdigest()
    if ((user_in == username) and (pwd_hash == password)):
        return 1
    else:
        return 0

def overwriteUserInfo(username,password):
    h = hashlib.sha256(password)
    hashword = h.hexdigest()
    f = open("bin/userinfo.txt","w")
    print(username+"\n"+hashword)
    f.write(username+"\n"+hashword)
    f.close()
    return