
class SimpleCommand(gdb.Command):
	def __init__(self):
        # This registers our class as "simple_command"
		super(SimpleCommand, self).__init__("zcmd", gdb.COMMAND_DATA)
    
	def invoke(self, arg, from_tty):
		"Specify an optional arena addr: print_mstats main_arena=0x12345"
	
		if SIZE_SZ == 0:
			retrieve_sizesz()
		try:
			mp = gdb.selected_frame().read_var('mp_')
			if arg.find("main_arena") == -1:
				main_arena = gdb.selected_frame().read_var('main_arena')
				main_arena_address = main_arena.address
			else:
				arg = arg.split()
				for item in arg:
					if item.find("main_arena") != -1:
						if len(item) < 12:
							print_error("Malformed main_arena parameter")
							return
						else:
							main_arena_address = int(item[11:],16)
		except RuntimeError:
			print_error("No frame is currently selected.")
			return
		except ValueError:
			print_error("Debug glibc was not found.")
			return
	
		if main_arena_address == 0:
			print_error("Invalid main_arena address (0)")
			return
	
		in_use_b = mp['mmapped_mem']
		system_b = in_use_b
	
		arena = 0
		while(1):
			ar_ptr = malloc_state(main_arena_address)
			mutex_lock(ar_ptr)
	
			print_title("Malloc Stats")
	
			# account for top
			avail = chunksize(malloc_chunk(top(ar_ptr), inuse=True, \
					read_data=False))
			nblocks = 1
	
			nfastblocks = 0
			fastavail = 0
	
			# traverse fastbins
			for i in range(NFASTBINS):
				p = fastbin(ar_ptr, i)
				while p!=0:
					p = malloc_chunk(p, inuse=False)
					nfastblocks += 1
					fastavail += chunksize(p)
					p = p.fd
	
			avail += fastavail
	
			# traverse regular bins
			for i in range(1, NBINS):
				b = bin_at(ar_ptr, i)
				p = malloc_chunk(first(malloc_chunk(b,inuse=False)),inuse=False)
	
				while p.address != int(b):
					nblocks += 1
					avail += chunksize(p)
					p = malloc_chunk(first(p), inuse=False)
	
			print_header("Arena {}:\n".format(arena))
			print("{:16} = ".format("system bytes"), end='')
			print_value("{:#x}".format(ar_ptr.max_system_mem))
			print("{:16} = ".format("in use bytes"), end='')
			print_value("{:#x}".format(ar_ptr.max_system_mem - avail))
	
			system_b += ar_ptr.max_system_mem
			in_use_b += (ar_ptr.max_system_mem - avail)
	
			mutex_unlock(ar_ptr)
			if ar_ptr.next == main_arena_address:
				break
			else:
				ar_ptr = malloc_state(ar_ptr.next)
				arena += 1
	
		print_header("\nTotal (including mmap):\n")
		print("{:16} = ".format("system bytes"), end='')
		print_value("{:#x}".format(system_b))
		print("{:16} = ".format("in use bytes"), end='')
		print_value("{:#x}".format(in_use_b))
		print("{:16} = ".format("max system bytes"), end='')
		print_value("{:#x}".format(mp['max_total_mem']))
		print("{:16} = ".format("max mmap regions"), end='')
		print_value("{:#x}".format(mp['max_n_mmaps']))
		print("{:16} = ".format("max mmap bytes"), end='')
		print_value("{:#x}".format(mp['max_mmapped_mem']))
# This registers our class to the gdb runtime at "source" time.
SimpleCommand()
