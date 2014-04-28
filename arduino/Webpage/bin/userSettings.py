def checkUserInfo(user_in, pass_in):
    f = open("bin/userinfo.txt","r")
    username = f.readline().rstrip('\n')
    password = f.readline().rstrip('\n')
    f.close()
    if ((user_in == username) and (pass_in == password)):
        return 1
    else:
        return 0

def overwriteUserInfo(username,password):
    f = open("bin/userinfo.txt","w")
    f.write(username+"\n"+password)
    f.close()
    return