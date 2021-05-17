#Gavin.20201218
#User-defined cmd in gdb.
import re
class FindObj(gdb.Command):
	def __init__(self):
	    super(FindObj, self).__init__("zfindobj", gdb.COMMAND_DATA)

	def invoke(self, arg, from_tty):
		cout="NONE"
		p = re.compile(".*zgtestlock.*")
		for idx in range(10):
			inferiors = gdb.inferiors()
			for inferior in inferiors:
				for thread in inferior.threads():
					gtid = thread.num
					if gtid != 2:
						continue
					# Change to our threads context
					thread.switch()
					# Take a human readable copy of the backtrace, we'll need this for display later.
					#o = gdb.execute('bt', to_string=True)
	# Reset the gdb frame context to the "latest" frame.
					gdb.newest_frame()
					# Now, work down the frames.
					cur_frame = gdb.selected_frame()
					#print(dir(cur_frame))
					if arg:
						var = arg
					else:
						var = "obj"
					try:
						obj=cur_frame.read_var(var)
						out=gdb.execute("x/s *(long long) ((char *)*(unsigned long long *)((char *)*(long ){}+8)+8) + (char *)*(unsigned long long *)((char *)*(long ){}+8)+0xA".format(obj,obj),to_string=True)
						m=p.match(out)
						if m:
							print("!!!Matched!!!")
							print(out)
							print(cout)
							return
						print(out)
					except:
						pass
			cout=gdb.execute("c",to_string=True)
FindObj()
