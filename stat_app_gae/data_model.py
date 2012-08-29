from google.appengine.ext import db

class Client(db.Model):
    """client record"""
    client_id = db.StringProperty()
    first_activity = db.DateTimeProperty(auto_now_add=True)
    last_activity = db.DateTimeProperty()
    activity_count = db.IntegerProperty()

class Heartbeat(db.Model):
    """client heartbeat record"""
    client = db.ReferenceProperty(Client)
    ip = db.StringProperty()
    date = db.DateTimeProperty(auto_now_add=True)
