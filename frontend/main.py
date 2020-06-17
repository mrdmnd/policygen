from portunus import create_app, db
from portunus.models import User, Route

app = create_app()

#db.drop_all()
#db.create_all()

# Add the database instance and models to the shell session invoked with `flask shell`
@app.shell_context_processor
def make_shell_context():
    return {'db': db, 'User': User, 'Route': Route}
