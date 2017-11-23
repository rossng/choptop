from flask import _app_ctx_stack, jsonify
from choptop import app

def get_model():
    appContext = _app_ctx_stack.top
    choptop = getattr(appContext, "ChopTop", None)
    return choptop


@app.route('/position')
def get_position(self):
    choptop = get_model()
    return jsonify(choptop.finger_position)