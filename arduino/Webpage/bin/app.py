import web, os
from web import form

from subprocess import Popen, PIPE

import sys
sys.path.append('/mnt/sda1/arduino')
import message_passing

urls = (
  '/', 'Index',
  '/settings', 'Settings'
)

app = web.application(urls, globals())

cur_temp_form = form.Form(
	form.Textbox("temperature", description="temperature"),
	form.Button("update", type="submit", description="Update")
)

render = web.template.render('templates/')

class Index(object):
    def GET(self):
        return render.index()

class Settings(object):
	def GET(self):
		out1 = message_passing.getRoomTemp()
		outA = message_passing.getTargetTemp()
		#proc = Popen('python ./static/pull_temp.py', shell=True, stdout=PIPE)
		#proc.wait()
		#(out1, out2) = proc.communicate()
		#proc2 = Popen('python ./static/pull_target_temp.py', shell=True, stdout=PIPE)
		#(outA, outB) = proc2.communicate()
		# do $:f.render() in the template
		f = cur_temp_form()
		return render.settings(f, out1, outA)
		

if __name__ == "__main__":
    app.run()
