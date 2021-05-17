
class SimpleCommand(gdb.Command):
	def __init__(self):
        # This registers our class as "simple_command"
		super(SimpleCommand, self).__init__("zcmd", gdb.COMMAND_DATA)
    
	def invoke(self, arg, from_tty):
		for inferior in gdb.inferiors():
			for head_thread in inferior.threads():
				head_thread.switch()
				# print(help(head_thread))
				head_thread.switch()
				# These are the OS pid references.
				(tpid, lwpid, tid) = head_thread.ptid
				# This is the gdb thread number
				gtid = head_thread.num
				print("tpid %s, lwpid %s, tid %s, gtid %s" % (tpid, lwpid, tid, gtid))

				# Reset the gdb frame context to the "latest" frame.
				gdb.newest_frame()
				# Now, work down the frames.
				cur_frame = gdb.selected_frame()
				print(dir(cur_frame))
				out=cur_frame.read_var("env")
				print(out)
				# while cur_frame is not None:
				# 	print(cur_frame.name())
				# 	# get the next frame down ....
				# 	cur_frame = cur_frame.older()

# This registers our class to the gdb runtime at "source" time.
SimpleCommand()
