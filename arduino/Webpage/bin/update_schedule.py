import sys
sys.path.append('/mnt/sda1/arduino/Scheduler')
import schedule_processing as sch

def print_data(tag,date,time,temp,repeat, f):
    if (tag and date and time and temp and repeat and f):
        f.write(str(tag+"::"+date+"::"+time+"::"+temp+"::"+repeat+";"))

def updateSchedule(arrLen, form):
    p_file = open("/mnt/sda1/arduino/Scheduler/schedule.txt", "w")
    
    #tag = form['name_0']
    #if (tag):
    #    print tag
    #print_data(test,"x","x","x","x",p_file)
    
    for x in range(0, arrLen-1):
        tag = str(form["name_"+str(x)])
        if (tag and tag!=''):
            repeat = str(form["sel"+str(x)])
            if (repeat == 'daily'):
                date = "NA"
            elif (repeat == 'weekly'):
                date = str(form["date_w"+str(x)])
            elif (repeat == 'monthly'):
                date = str(form["date_m"+str(x)])
            else:
                date = str(form["date_o"+str(x)])
            hr = form["hr_"+str(x)]
            min = form["min_"+str(x)]
            time = str(hr)+":"+str(min)
            temp = str(form["temp_"+str(x)])
            #if (tag and repeat and time and date and temp):
            #print (tag, date, time, temp, repeat)
            if (repeat == 'daily'):
                print_data(tag,'NA',time,temp,'d',p_file)
            elif (repeat == 'weekly'):
                days = {"Sun":'1',"Mon":'2',"Tue":'3',"Wed":'4',"Thu":'5',"Fri":'6',"Sat":'7'}
                date = days[date]
                print_data(tag,date,time,temp,'w',p_file)
            elif (repeat == 'monthly'):
                print_data(tag,date,time,temp,'m',p_file)
            else:
                dsp = date.split('/')
                if (len(dsp) > 1):
                    date = dsp[0]+"/"+dsp[1]
                print_data(tag,date,time,temp,'o',p_file)
    
    p_file.close()
    sch.makeCron()