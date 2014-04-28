import web
import re
import base64
import sys
import userSettings
from web import form
sys.path.append('/mnt/sda1/arduino')
from message_passing import getRoomTemp, getTargetTemp

urls = (
    '/','Index',
    '/login','Login',
    '/settings', 'Settings',
    '/schedule', 'Schedule'
)

app = web.application(urls,globals())

render = web.template.render('templates/')

class Index:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            current = str(getRoomTemp())
            target = str(getTargetTemp())
            return render.index(current,target)
        else:
            raise web.seeother('/login')

class Settings:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            try:
                zc_f = open("/mnt/sda1/arduino/WeatherApp/zipcode.txt","r")
                zip_code = zc_f.readline()
                zc_f.close()
            except:
                zc_f = open("/mnt/sda1/arduino/WeatherApp/zipcode.txt","w")
                zip_code = '44106'
                zc_f.write(zip_code)
                zc_f.close
            return render.settings(zip_code)
        else:
            raise web.seeother('/login')
    
    def POST(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            import cgi
            form = web.input(myfile={})
            try:
                z_code = str(form["zc"])
                zc_file = open("/mnt/sda1/arduino/WeatherApp/zipcode.txt","w")
                zc_file.write(z_code)
                zc_file.close()
            except:
                pass
            try:
                c_u = str(form["cu"])
                c_p = str(form["cp"])
                n_u = str(form["nu"])
                n_p = str(form["np"])
                match = userSettings.checkUserInfo(c_u,c_p)
                if (match):
                    userSettings.overwriteUserInfo(n_u,n_p);
            except:
                pass
            raise web.seeother('/settings')
        else:
            raise web.seeother('/login')

class Schedule:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            # Access the schedule file
            sch_file = open("/mnt/sda1/arduino/Scheduler/schedule.txt", "r")
            sch_str = sch_file.read()
            sch_file.close()
            if not sch_str and sch_str != "":
                return render.schedule(0, [])
            setting_list = sch_str.split(';')
            sch_arr = []
            for x in setting_list:
                triple = x.split('::')
                # Now in the format (tag, day/date, time, temp, repeat status)
                if (triple[0] and triple[1] and triple[2] and triple[3] and triple[4]):
                    sch_arr.append([triple[0], triple[1], triple[2], triple[3], triple[4]])
            arrLen = len(sch_arr)
            return render.schedule(arrLen, sch_arr)
        else:
            raise web.seeother('/login')
    
    def POST(self):
        # Same as get, except overwrite the schedule file????????????????
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            import cgi
            import update_schedule
            #Access the schedule file the first time
            sch_f1 = open("/mnt/sda1/arduino/Scheduler/schedule.txt", "r")
            sch_st1 = sch_f1.read()
            sch_f1.close()
            set_list = sch_st1.split(';')
            set_len = len(set_list)+5
            #All we needed was the length
            form = web.input(myfile={})
            update_schedule.updateSchedule(set_len, form)
            return web.seeother('/schedule')
        else:
            raise web.seeother('/login')

class Login:
    def GET(self):
        auth = web.ctx.env.get('HTTP_AUTHORIZATION')
        authreq = False
        if auth is None:
            authreq = True
        else:
            auth = re.sub('^Basic ','',auth)
            username,password = base64.decodestring(auth).split(':')
            match = userSettings.checkUserInfo(username,password)
            if (match):
                raise web.seeother('/')
            else:
                authreq = True
        if authreq:
            web.header('WWW-Authenticate','Basic realm="Auth example"')
            web.ctx.status = '401 Unauthorized'
            return

if __name__=='__main__':
    app.run()
