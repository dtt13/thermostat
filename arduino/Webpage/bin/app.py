import web
import re
import base64
from web import form

urls = (
    '/','Index',
    '/login','Login',
    '/settings', 'Settings',
    '/schedule', 'Schedule'
)

app = web.application(urls,globals())

allowed = (
    ('user1','pass1'),
    ('admin', 'password')
)

temp_schedule_form = form.Form(
    form.Textbox("day", description="day"),
    form.Textbox("time", description="time"), 
    form.Textbox("temp", description="temp")
)

render = web.template.render('templates/')

class Index:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            return render.index()
        else:
            raise web.seeother('/login')

class Settings:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            return render.settings()
        else:
            raise web.seeother('/login')

class Schedule:
    def GET(self):
        if web.ctx.env.get('HTTP_AUTHORIZATION') is not None:
            # Access the schedule file
            sch_file = open("bin/schedule.txt", "r")
            sch_str = sch_file.read()
            sch_file.close()
            setting_list = sch_str.split(';')
            sch_arr = []
            for x in setting_list:
                triple = x.split('::')
                # Now in the format (day/date, time, temp, repeat status)
                if (triple[0] and triple[1] and triple[2] and triple[3] and triple[4]):
                    sch_arr.append([triple[0], triple[1], triple[2], triple[3], triple[4]])
            arrLen = len(sch_arr)
            f = temp_schedule_form()
            return render.schedule(f, arrLen, sch_arr)
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
            if (username,password) in allowed:
                raise web.seeother('/')
            else:
                authreq = True
        if authreq:
            web.header('WWW-Authenticate','Basic realm="Auth example"')
            web.ctx.status = '401 Unauthorized'
            return

if __name__=='__main__':
    app.run()
