import mitmproxy.http
from mitmproxy import ctx
class Joker:
    def request(self, flow: mitmproxy.http.HTTPFlow):
        ctx.log.info('get request')
        return
    def response(self, flow: mitmproxy.http.HTTPFlow):
        text = flow.response.get_text()
        text = text.replace('tsinghua','peking')
        ctx.log.info('change response')
        flow.response.set_text(text)

addons = [
    Joker()
]