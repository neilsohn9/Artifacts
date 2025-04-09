#!/usr/bin/python

"""
Simple Voting HTTP server with MySQL database

Setup in Ubuntu:
$ sudo apt-get install python-pip python-dev libmysqlclient-dev
$ pip install MySQL-python
"""

import MySQLdb
import cgi
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

# Server and database combination
PORT_NUMBER = 8080
DB_HOST = 'localhost'
DB_USER = 'root'
DB_PASS = 'root'
DB_NAME = 'vote'


class VoteHandler(BaseHTTPRequestHandler):
    """
    HTTP request handler for simple voting
    """

    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(get_vote_form_html())
        return

    def do_POST(self):
        form = cgi.FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
        )

        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        try:
            candidate = form.getvalue('candidate')
            state = form.getvalue('state')
            if inc_vote(candidate, state) == 1:
                html = get_vote_form_html('Thanks for your vote!', message_color='green')
            else:
                html = get_vote_form_html('Vote error. Invalid candidate and/or state', message_color='red')
        except Exception as e:
            print(e)
            html = get_vote_form_html('Server error. Please contact support', message_color='red')

        self.wfile.write(html)
        return


def get_vote_form_html(message_html=None, message_color='green'):
    """
    Generate HTML with form for voting
    """
    candidate_html = get_radio_group_html('candidate', get_distinct_vote('candidate'))
    state_html = get_radio_group_html('state', get_distinct_vote('state'))
    html_form = """
<html>
<head><title>Voting App</title></head>
<body>
"""
    if message_html:
        html_form += """
        <p style="color:{}">{}</p>
        """.format(message_color, message_html)

    html_form += """
<form action="" method="POST">
    <table>
        <tr><td>Candidates:</td><td>{}</td></tr>
        <tr><td>States:</td><td>{}</td></tr>
        <tr><td><input type="submit" value="Submit"/></td></tr>
    </table>
</form>
""".format(candidate_html, state_html)

    html_form += """
    </body>
    </html>
    """
    return html_form


def get_distinct_vote(col):
    """
    Get distinct vote column
    """
    if not db:
        raise Exception('Connection not opened')
    else:
        cursor = db.cursor()
        cursor.execute('SELECT DISTINCT {} FROM vote'.format(col))
        results = cursor.fetchall()
        return sorted([row[0] for row in results])


def inc_vote(candidate, state):
    """
    Increase vote for certain candidate and state by 1
    """
    if not db:
        raise Exception('Connection not opened')
    else:
        try:
            cursor = db.cursor()
            # use parameterized query to prevent sql injection
            affected_rows = cursor.execute("UPDATE vote SET total_votes = total_votes + 1 WHERE candidate = %s AND state = %s", [candidate, state])
            db.commit()
            return affected_rows
        except Exception as e:
            db.rollback()
            raise Exception('Database update failed')
    return 0


def get_radio_group_html(group_name, values):
    html = []
    for val in values:
        if not html:
            default = 'checked'
        else:
            default = ''
        html.append('<input type="radio" name="{0}" value="{1}" {2}/> {1}'.format(group_name, val, default))
    return ' '.join(html)


if __name__ == '__main__':
    try:
        # connect to database
        db = MySQLdb.connect(DB_HOST, DB_USER, DB_PASS, DB_NAME)
        print 'Connected to database {}@{}'.format(DB_NAME, DB_HOST)
        # start HTTP server
        server = HTTPServer(('', PORT_NUMBER), VoteHandler)
        print 'Server is started and accessible on http://localhost:{}'.format(PORT_NUMBER)
        print 'Press CTRL+C to shutdown..'
        server.serve_forever()

    except KeyboardInterrupt:
        print 'Shutting down the web server'
        # shutdown server
        server.socket.close()
        # close db connection
        db.close()
