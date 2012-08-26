from datetime import datetime

from google.appengine.ext import webapp
from google.appengine.ext import db

from data_model import Client
from data_model import Heartbeat


class MainHandler(webapp.RequestHandler):
    def get(self):
        self.response.out.write('<html><body><h1>UrTConnector usage statistics</h1>')

        clients = Client.all()
        self.response.out.write('<p>Unique clients: <b>%d</b></p>' % (clients.count()))
        heartbeats = Heartbeat.all()
        self.response.out.write('<p>Total launch count: <b>%d</b></p>' % (heartbeats.count()))

        self.response.out.write('<h2>Launches:</h2><table border="1">')
        self.response.out.write('<tr><td><b>Client ID</b></td><td><b>IP address</b></td><td><b>Date</b></td></tr>')
        heartbeats = Heartbeat.gql("ORDER BY date")
        for beat in heartbeats:
            self.response.out.write('<tr><td>%s</td><td>%s</td><td>%s</td></tr>' % (beat.client.client_id, beat.ip, beat.date))
        self.response.out.write('</table>')

        self.response.out.write('</body></html>')

class HeartbeatHandler(webapp.RequestHandler):
    def get(self):
        cid = self.request.get('id')
        if not cid:
            self.response.out.write('Client id is required')
            return

        # get client record
        client = Client.gql("WHERE client_id = :1", cid).get()
        if not client:
            # creating new client record
            client = Client()
            client.client_id = cid
            client.activity_count = 0

        client.activity_count += 1
        client.last_activity = datetime.now()
        client.put()

        beat = Heartbeat()
        beat.client = client
        beat.ip = self.request.remote_addr
        beat.put()
        self.response.out.write('Heartbeat was recorded ip:"%s" id:"%s"' % (beat.ip, cid))


app = webapp.WSGIApplication([
    ('/', MainHandler), ('/heartbeat', HeartbeatHandler)],
    debug=True)

