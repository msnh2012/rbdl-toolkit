#include "EmbeddedPython.h"

#include <pybind11/embed.h>

const char *INIT_SCRIPT =
    "from codeop import CommandCompiler, compile_command\n"
    "class InteractiveSocket():\n"
    "  def __init__(self, sock, locals=None, filename='console'):\n"
    "    if locals is None:\n"
    "      locals = {\"__name__\": \"__console__\", \"__doc__\": None}\n"
    "    locals[\"__builtins__\"] = {\"print\": self.write }\n"
    "    self.locals = locals\n"
    "    self.compile = CommandCompiler()\n"

    "    self.filename = filename\n"
    "    self.resetbuffer()\n"
    "    sys.stderr = sock\n"
    "    sys.stdout = sock\n"

    "    self.sock = sock\n"

    "  def runsource(self, source, filename=\"<input>\", symbol=\"single\"):\n"
    "    try:\n"
    "        code = self.compile(source, filename, symbol)\n"
    "    except (OverflowError, SyntaxError, ValueError):\n"
    "        # Case 1\n"
    "        self.showsyntaxerror(filename)\n"
    "        return False\n"
    "    if code is None:\n"
    "      # Case 2\n"
    "      return True\n"
    "    # Case 3\n"
    "    self.runcode(code)\n"
    "    return False\n"

    "  def runcode(self, code):\n"
    "    try:\n"
    "        exec(code, self.locals)\n"
    "    except SystemExit:\n"
    "        raise\n"
    "    except:\n"
    "        self.showtraceback()\n"

    "  def showsyntaxerror(self, filename=None):\n"
    "    type, value, tb = sys.exc_info()\n"
    "    sys.last_type = type\n"
    "    sys.last_value = value\n"
    "    sys.last_traceback = tb\n"
    "    if filename and type is SyntaxError:\n"
    "        # Work hard to stuff the correct filename in the exception\n"
    "        try:\n"
    "            msg, (dummy_filename, lineno, offset, line) = value.args\n"
    "        except ValueError:\n"
    "            # Not the format we expect; leave it alone\n"
    "            pass\n"
    "        else:\n"
    "            # Stuff in the right filename\n"
    "            value = SyntaxError(msg, (filename, lineno, offset, line))\n"
    "            sys.last_value = value\n"
    "    if sys.excepthook is sys.__excepthook__:\n"
    "        lines = traceback.format_exception_only(type, value)\n"
    "        self.write(''.join(lines))\n"
    "    else:\n"
    "        # If someone has set sys.excepthook, we let that take precedence\n"
    "        # over self.write\n"
    "        sys.excepthook(type, value, tb)\n"

    "  def showtraceback(self):\n"
    "    sys.last_type, sys.last_value, last_tb = ei = sys.exc_info()\n"
    "    sys.last_traceback = last_tb\n"
    "    try:\n"
    "        lines = traceback.format_exception(ei[0], ei[1], last_tb.tb_next)\n"
    "        if sys.excepthook is sys.__excepthook__:\n"
    "            self.write(''.join(lines))\n"
    "        else:\n"
    "            # If someone has set sys.excepthook, we let that take precedence\n"
    "            # over self.write\n"
    "            sys.excepthook(ei[0], ei[1], last_tb)\n"
    "    finally:\n"
    "        last_tb = ei = None\n"

    "  def interact(self, banner=None, exitmsg=None):\n"
    "    try:\n"
    "        sys.ps1\n"
    "    except AttributeError:\n"
    "        sys.ps1 = \">>> \"\n"
    "    try:\n"
    "        sys.ps2\n"
    "    except AttributeError:\n"
    "        sys.ps2 = \"... \"\n"
    "    cprt = 'Type \"help\", \"copyright\", \"credits\" or \"license\" for more information.'\n"
    "    if banner is None:\n"
    "        self.write('Python %s on %s\\n%s\\n(%s)\\n' % (sys.version, sys.platform, cprt, self.__class__.__name__))\n"
    "    elif banner:\n"
    "        self.write(\"%s\\n\" % str(banner))\n"
    "    more = 0\n"
    "    while 1:\n"
    "        try:\n"
    "            if more:\n"
    "                prompt = sys.ps2\n"
    "            else:\n"
    "                prompt = sys.ps1\n"
    "            try:\n"
    "                line = self.raw_input(prompt)\n"
    "            except EOFError:\n"
    "                self.write(\"\\n\")\n"
    "                break\n"
    "            else:\n"
    "                more = self.push(line)\n"
    "        except KeyboardInterrupt:\n"
    "            self.write(\"\\nKeyboardInterrupt\\n\")\n"
    "            self.resetbuffer()\n"
    "            more = 0\n"
    "    if exitmsg is None:\n"
    "        self.write('now exiting %s...\\n' % self.__class__.__name__)\n"
    "    elif exitmsg != '':\n"
    "        self.write('%s\\n' % exitmsg)\n"

    "  def push(self, line):\n"
    "    self.buffer.append(line)\n"
    "    source = \"\\n\".join(self.buffer)\n"
    "    more = self.runsource(source, self.filename)\n"
    "    if not more:\n"
    "        self.resetbuffer()\n"
    "    return more\n"

    "  def resetbuffer(self):\n"
    "    self.buffer = []\n"

    "  def write(self, data):\n"
    "    if not self.sock.is_closed():\n"
    "      self.sock.write(data.encode('ascii'))\n"
    "      self.sock.flush()\n"

    "  def raw_input(self, prompt='>>>'):\n"
    "    if (self.sock.is_closed()):\n"
    "      raise EOFError('Socket closed')\n"

    "    self.write(prompt)\n"
    "    r = self.sock.readline()\n"
    "    return r\n"

    "repl_scope = dict(globals(), **locals())\n"
    "isock = InteractiveSocket(ts, repl_scope)\n"
    "isock.interact()\n";

class ToolkitSocket {
	private:
		QLocalSocket *s;
	public:
		ToolkitSocket(QLocalSocket* s) : s(s) {}
		~ToolkitSocket() {
			s->disconnect();
			s->deleteLater();
		}

		void write(std::string str) {
			s->write(str.data(), str.size());
		}

		std::string read(int len) {
			while(!s->waitForReadyRead()){}
			QByteArray data = s->read(len);
			return QString(data).toStdString();
		}

		std::string readline() {
			while(!s->waitForReadyRead()){}
			auto line = s->readLine(1024);
			return QString(line).toStdString();
		}

		void flush() {
			s->flush();
		}

		bool is_closed() {
			return !s->isOpen();
		}
};

PYBIND11_EMBEDDED_MODULE(toolkitsocket, m) {
	pybind11::class_<ToolkitSocket>(m, "ToolkitSocket")
		.def(pybind11::init<QLocalSocket*>())
		.def("write", &ToolkitSocket::write)
		.def("read", &ToolkitSocket::read)
		.def("readline", &ToolkitSocket::readline)
		.def("is_closed", &ToolkitSocket::is_closed)
		.def("flush", &ToolkitSocket::flush);
}

EmbeddedPython::EmbeddedPython(ToolkitApp* toolkit) : toolkit(toolkit) {
}

EmbeddedPython::~EmbeddedPython() {
}

void EmbeddedPython::startPythonShell(QLocalSocket* socket) {
	auto guard = new pybind11::scoped_interpreter();
	pybind11::module::import("toolkitsocket");
	using namespace pybind11::literals;

	ToolkitSocket *s = new ToolkitSocket(socket);
	pybind11::object ts = pybind11::cast(s);
	pybind11::module_ sys = pybind11::module_::import("sys");
	pybind11::module_ traceback = pybind11::module_::import("traceback");
	auto locals = pybind11::dict(
		"ts"_a = ts,
		"sys"_a = sys,
		"traceback"_a = traceback
	);
	pybind11::exec(INIT_SCRIPT, locals);
	delete guard;
}
