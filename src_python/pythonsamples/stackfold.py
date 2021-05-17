import gdb
class StackFold(gdb.Command):
	def __init__(self):
	    super(StackFold, self).__init__("stackfold", gdb.COMMAND_DATA)

	# def invoke(self, arg, from_tty):
    # # An inferior is the 'currently running applications'. In this case we only
    # # have one.
	# 	# inferiors = gdb.inferiors()
	# 	# for inferior in inferiors:
	# 	# 	print(inferior)
	# 	# 	print(dir(inferior))
	# 	# 	print(help(inferior))
	# 	inferiors = gdb.inferiors()
	# 	for inferior in inferiors:
	# 		for head_thread in inferior.threads():
	# 			head_thread.switch()
	# 			# print(help(head_thread))
	# 			head_thread.switch()
	# 			# These are the OS pid references.
	# 			(tpid, lwpid, tid) = head_thread.ptid
	# 			# This is the gdb thread number
	# 			gtid = head_thread.num
	# 			print("tpid %s, lwpid %s, tid %s, gtid %s" % (tpid, lwpid, tid, gtid))

	# 			# Reset the gdb frame context to the "latest" frame.
	# 			gdb.newest_frame()
	# 			# Now, work down the frames.
	# 			cur_frame = gdb.selected_frame()
	# 			while cur_frame is not None:
	# 				print(cur_frame.name())
	# 				# get the next frame down ....
	# 				cur_frame = cur_frame.older()
	def invoke(self, arg, from_tty):
		# An inferior is the 'currently running applications'. In this case we only
		# have one.
		stack_maps = {}
		# This creates a dict where each element is keyed by backtrace.
		# Then each backtrace contains an array of "frames"
		#
		inferiors = gdb.inferiors()
		for inferior in inferiors:
			for thread in inferior.threads():
				# Change to our threads context
				thread.switch()
				# Get the thread IDS
				(tpid, lwpid, tid) = thread.ptid
				gtid = thread.num
				# Take a human readable copy of the backtrace, we'll need this for display later.
				o = gdb.execute('bt', to_string=True)
				# Build the backtrace for comparison
				backtrace = []
				gdb.newest_frame()
				cur_frame = gdb.selected_frame()
				while cur_frame is not None:
					backtrace.append(cur_frame.name())
					cur_frame = cur_frame.older()
				# Now we have a backtrace like ['pthread_cond_wait@@GLIBC_2.3.2', 'lazy_thread', 'start_thread', 'clone']
				# dicts can't use lists as keys because they are non-hashable, so we turn this into a string.
				# Remember, C functions can't have spaces in them ...
				s_backtrace = ' '.join(backtrace)
				# Let's see if it exists in the stack_maps
				if s_backtrace not in stack_maps:
					stack_maps[s_backtrace] = []
				# Now lets add this thread to the map.
				stack_maps[s_backtrace].append({'gtid': gtid, 'tpid' : tpid, 'bt': o} )
		# Now at this point we have a dict of traces, and each trace has a "list" of pids that match. Let's display them
		for smap in stack_maps:
			# Get our human readable form out.
			o = stack_maps[smap][0]['bt']
			for t in stack_maps[smap]:
				# For each thread we recorded
				print("Thread %s (LWP %s))" % (t['gtid'], t['tpid']))
			print(o)
StackFold()
