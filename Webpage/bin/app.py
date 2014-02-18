import web, os
from web import form

from subprocess import Popen, PIPE

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
		proc = Popen('python ./static/testing.py', shell=True, stdout=PIPE)
		#proc.wait()
		(out1, out2) = proc.communicate()
		# do $:f.render() in the template
		f = cur_temp_form()
		return render.settings(f, out1)
		

if __name__ == "__main__":
    app.run()
