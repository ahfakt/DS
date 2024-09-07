#(gdb) source ./printers.py
import gdb.printing # type: ignore
import re


class Printer(object):
	class SubPrettyPrinter(gdb.printing.SubPrettyPrinter):
		def __init__(self, subname, regexp, gen_printer):
			super(Printer.SubPrettyPrinter, self).__init__(subname)
			self.regexp = regexp
			self.gen_printer = gen_printer
			self.compiled_re = re.compile(regexp)

	class PrettyPrinter(gdb.printing.PrettyPrinter):
		def __init__(self, name):
			super(Printer.PrettyPrinter, self).__init__(name, [])

		def __call__(self, val):
			typename = gdb.types.get_basic_type(val.type).tag
			if not typename:
				typename = val.type.name
			if not typename:
				typename = str(val.type)

			for printer in self.subprinters:
				if printer.enabled and printer.compiled_re.search(typename):
					return printer.gen_printer(val)

			return None

	Collection = {}

	def __init__(self, name, subname, regexp):
		if name not in Printer.Collection:
			Printer.Collection[name] = self.PrettyPrinter(name)
			gdb.printing.register_pretty_printer(gdb.current_progspace(), Printer.Collection[name], replace = True)
		self.name    = name
		self.subname = subname
		self.regexp  = regexp

	def __call__(self, gen_printer):
		Printer.Collection[self.name].subprinters.append(self.SubPrettyPrinter(self.subname, self.regexp, gen_printer))


@Printer('DS', 'CQueue<T>', '^DS::CQueue<.*>$')
class DSCQueue(gdb.ValuePrinter):
	class _iterator(object):
		def __init__(self, head):
			self.curr  = head
			self.index = 0

		def __iter__(self):
			return self

		def __next__(self):
			if self.curr == 0 or self.curr == self.curr.address:
				raise StopIteration
			index = self.index
			item  = self.curr['val']
			self.index += 1
			self.curr  = self.curr['next']
			return (f"[{index}]", item)

	def __init__(self, val):
		self.__val = val

	def children(self):
		node_t = gdb.lookup_type('DS::CQNode<' + str(self.__val.type.template_argument(0)) + '>')
		counted_ptr_t = gdb.lookup_type('DS::CountedPtr<DS::CQNode<' + str(self.__val.type.template_argument(0)) + '>>')
		return self._iterator(self.__val['mHead'].cast(counted_ptr_t)['ptr'].cast(node_t.pointer()))

	def to_string(self):
		return f"[{self.__val['mSize']}]"

	def display_hint(self):
		return 'array'


@Printer('DS', 'Holder<T>', '^DS::Holder<.*>$')
class DSHolder(gdb.ValuePrinter):
	def __init__(self, val):
		self.__val = val

	def to_string(self):
		return self.__val.address.cast(self.__val.type.template_argument(0).pointer()).dereference()


@Printer('DS', 'List<T>', '^DS::List<.*>$')
class DSList(gdb.ValuePrinter):
	class _iterator(object):
		def __init__(self, head):
			self.curr  = head
			self.index = 0

		def __iter__(self):
			return self

		def __next__(self):
			if self.curr == 0:
				raise StopIteration
			index = self.index
			item  = self.curr['val']
			self.index += 1
			self.curr  = self.curr['next']
			return (f"[{index}]", item)

	def __init__(self, val):
		self.__val = val

	def children(self):
		return self._iterator(self.__val['mHead'])

	def to_string(self):
		return f"[{self.__val['mSize']}]"

	def display_hint(self):
		return 'array'


@Printer('DS', 'Vector<T>', '^DS::Vector<.*>$')
class DSVector(gdb.ValuePrinter):
	class _iterator(object):
		def __init__(self, beg, end):
			self.curr  = beg
			self.end   = end
			self.index = 0

		def __iter__(self):
			return self

		def __next__(self):
			if self.curr == self.end:
				raise StopIteration
			index = self.index
			item  = self.curr.dereference()
			self.index += 1
			self.curr  += 1
			return (f"[{index}]", item)

	def __init__(self, val):
		self.__val = val

	def children(self):
		return self._iterator(self.__val['mHead'], self.__val['mHead'] + self.__val['mSize'])

	def to_string(self):
		return f"[{self.__val['mSize']}/{self.__val['mCapacity']}]"

	def display_hint(self):
		return 'array'
