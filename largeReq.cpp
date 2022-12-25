#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>

namespace net       = boost::asio;
namespace beast     = boost::beast;
namespace http      = beast::http;
using tcp           = net::ip::tcp;
using socket_t      = tcp::socket;

template<
    bool isRequest,
    class SyncReadStream,
    class DynamicBuffer>
void
read_and_print_body(
    std::ostream& os,
    SyncReadStream& stream,
    DynamicBuffer& buffer,
    beast::error_code& ec)
{
    http::parser<isRequest, http::buffer_body> p;
    http::read_header(stream, buffer, p, ec);
    if(ec)
        return;
    while(! p.is_done())
    {
        char buf[512];
        p.get().body().data = buf;
        p.get().body().size = sizeof(buf);
        http::read(stream, buffer, p, ec);
        if(ec == http::error::need_buffer)
            ec = {};
        if(ec)
            return;
        os.write(buf, sizeof(buf) - p.get().body().size);
    }
}

int main() {
    std::string host = "173.203.57.63"; 
    auto const port  = "80";

    net::io_context ioc;
    tcp::resolver   resolver{ioc};

    socket_t s{ioc};
    net::connect(s, resolver.resolve(host, port));

    write(s, http::request<http::empty_body>{http::verb::get, "/", 11});

    beast::error_code  ec;
    beast::flat_buffer buf;

    read_and_print_body<false>(std::cout, s, buf, ec);
}


// g++ -std=c++20 -O2 -Wall -pedantic -pthread main.cpp && ./a.out
/*

g++ largeReq.cpp -I ../1.81.0/
[leanne@leanne-OptiPlex-9020:~/libs/boost/examples]
% ./a.out
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Frameset//EN">
<html>
<head>
    <title>Coliru</title>
    <script type="text/javascript">

      var _gaq = _gaq || [];
      _gaq.push(['_setAccount', 'UA-37220775-1']);
      _gaq.push(['_trackPageview']);

      (function() {
        var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
        ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
        var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
      })();


    </script>
</head>
<script type="text/javascript" language="javascript">

    var app = { };
    app.elements = {};

    document.write('<frameset rows="100%, 200" border="4" >');
    document.write(' <frame src="frame-top.html" name="top" scrolling="no" frameborder="1" />');
    document.write(' <frame src="frame-bottom.html" name="bottom" scrolling="no" frameborder="1" />');
    document.write('</frameset>');


    // Redirect www. to host domain.
    if (window.location.host.search(/^www./) === 0 || window.location.host.search(/^stacked/) === 0) {
      var host = window.location.hostname + "";
      host = host.replace(/^www./, "");
      host = host.replace(/^stacked/, "coliru.stacked");
      window.location.hostname = host;
    }

    window.onload = function () {


        String.prototype.trim = function () {
            return this.replace(/^\s+|\s+$/g, "");
        };

        app.defaultCmd = "g++ -std=c++20 -O2 -Wall -pedantic -pthread main.cpp && ./a.out";
                app.defaultSrc = "#include <iostream>\n#include <string>\n#include <vector>\n\ntemplate<typename T>\nstd::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)\n{\n    for (auto& el : vec)\n    {\n        os << el << ' ';\n    }\n    return os;\n}\n\nint main()\n{\n    std::vector<std::string> vec = {\n        \"Hello\", \"from\", \"GCC\", __VERSION__, \"!\" \n    };\n    std::cout << vec << std::endl;\n}\n"

        app.elements.editor.getSession().setValue(localStorage.getItem(window.location.pathname.replace(/^\//,"") + "src") || localStorage.getItem("src") || app.defaultSrc);
        app.elements.editor.commands.bindKeys({"Ctrl-l":null}) 
        app.elements.editor.commands.bindKeys({"Ctrl-t":null}) 
        app.elements.editor.commands.bindKeys({"Command-l":null}) 
        app.elements.editor.commands.addCommand({ name: 'Build and run', bindKey: {win: 'Ctrl-B',  mac: 'Command-B'}, exec: function(editor) { app.compileNow(); }, readOnly: true });
        app.elements.editor.commands.addCommand({ name: 'Disable Ctrl-S', bindKey: {win: 'Ctrl-S',  mac: 'Command-S'}, exec: function(editor) {}, readOnly: true });

        app.elements.cmd.value = function() {
            return localStorage.getItem(window.location.pathname.replace(/^\//,"") + "cmd") || localStorage.getItem("cmd") || app.defaultCmd;
        }();

        app.resetCommand = function() {
            app.elements.cmd.value = app.defaultCmd;
        };

        app.resetEditor = function() {
            app.elements.editor.getSession().setValue(app.defaultSrc);
        };

        app.enableUI = function(value) {
            app.elements.compileButton.disabled = !value;
            app.elements.postButton.disabled = !value;
            app.elements.editor.setReadOnly(!value);
            app.elements.editor.setReadOnly(!value);
            app.elements.cmd.disabled = !value;
            var colors = [ '#ffff00' ];
            if (parent.app.elements.fade_count === undefined)
                parent.app.elements.fade_count = 0;//Math.round(Math.random() * 10) % colors.length;

            if (!value) {
                var n = parent.app.elements.fade_count++ % colors.length;
                app.elements.fade.style.backgroundColor = colors[n]
                //app.elements.fade.style.backgroundImage = 'url(/random_image)';
            }
            else {
                app.elements.fade.style.backgroundColor = '#ffffff';
                //app.elements.fade.style.backgroundImage = 'none';
            }
            app.elements.fade.style.opacity = value ? "1.00" : "0.20";
        };
        app.enableUI(true);

        app.send = function (location, f) {
            app.enableUI(false);
            app.elements.cmd.value = app.elements.cmd.value.trim();
            localStorage.setItem(window.location.pathname.replace(/^\//,"") + "src", app.elements.editor.getValue());
            localStorage.setItem(window.location.pathname.replace(/^\//,"") + "cmd", app.elements.cmd.value);
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("POST", document.location.protocol + "//" + window.location.hostname + ":" + window.location.port + '/' + location, true);
            httpRequest.onreadystatechange = function () {
                if (httpRequest.readyState == 4) {
                    app.enableUI(true);
                    if (httpRequest.status == 200) {
                        app.lastResult = httpRequest.responseText;
                    } else {
                        app.lastResult = httpRequest.statusText;
                    }
                    f({src: app.elements.editor.getValue(), cmd: app.elements.cmd.value, output: app.lastResult});
                }
            };
            var post_data = JSON.stringify({
                "cmd" : app.elements.cmd.value,
                "src" : app.elements.editor.getValue()
            });
            httpRequest.send(post_data);
        };


        app.previousValue = "";

        app.feedback = function (msg) {
            if (!msg) return;
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("POST", document.location.protocol + "//" + window.location.hostname + ":" + window.location.port + '/feedback', true);
            httpRequest.onreadystatechange = function () {
                if (httpRequest.readyState == 4) {
                    if (httpRequest.status == 200) {
                        window.location = document.location.protocol + "//coliru.stacked-crooked.com/feedback";
                    }
                }
            };
            httpRequest.send(msg);

        };
        app.compileNow = function () {
            if (app.elements.compileButton.disabled) return;
            app.send("compile", function (obj) {
                var lines = obj.output.split(/\n/);
                
                var pre_open = "<div><pre class='f0' style='display: inline-block; padding: 0; margin: 0;' onclick='window.jumpToError(this)' onmouseover='window.highlightError(this, true)' onmouseout='window.highlightError(this, false)' style='margin:0; padding:0; '>";
                var pre_close = "</pre></div>";
                var text = "";
                var new_line = true;
                for (var i = 0; i !== lines.length; ++i) {
                    var line = lines[i];
                    if (line !== '') {
                        text += pre_open + line.replace(/&/, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + pre_close;
                    } else {
                        text += '<br/>';
                    }
                }
                app.elements.output.innerHTML = text;

                app.elements.cmd.value = obj.cmd;
                window.setTimeout(function() {
                    var ace_lines = app.editorDocument.getElementsByClassName("ace_line");
                    var i;
                    for (i = 0; i !== ace_lines.length; ++i) {
                        ace_lines[i].style.color = "inherit";
                    }
                    var output_lines = app.elements.output.textContent.split(/\n/);
                    for (i = 0; i != output_lines.length; ++i) {
                        try {
                            var line = output_lines[i];
                            if (line.search(/^main.cpp:\d+:\d+/) !== -1) {
                                var lineno = parseInt(line.split(":")[1]);
                                ace_lines[lineno - 1].style.color = "red";
                            }
                        }
                        catch (e) {
                            console.log(JSON.stringify(e));
                        }
                    }
                }, 100);
            });
        };

        app.postSample = function () {
            app.send("share", function (obj) {
                var url = window.location + "a/" + obj.output;
                window.open(url, '_self');
            });
        };
        app.previousValue = "";
    };
</script>
</html>

*/
