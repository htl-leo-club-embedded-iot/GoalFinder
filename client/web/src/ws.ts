type JsonValue = any;

class WSClient {
    private socket: WebSocket | null = null;
    private pending: Array<{resolve: (v: any)=>void, reject: (e:any)=>void}> = [];
    private listeners: Array<(msg: any)=>void> = [];

    connect() {
        if (this.socket && (this.socket.readyState === WebSocket.OPEN || this.socket.readyState === WebSocket.CONNECTING)) return;
        const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
        const url = `${protocol}//${location.host}/ws`;
        this.socket = new WebSocket(url);

        this.socket.addEventListener('open', () => {
            // no-op for now
        });

        this.socket.addEventListener('message', (ev) => {
            try {
                const msg = JSON.parse(ev.data);
                if (this.pending.length > 0) {
                    const p = this.pending.shift();
                    p?.resolve(msg);
                } else {
                    this.listeners.forEach(l => l(msg));
                }
            } catch (e) {
                // ignore non-json
            }
        });

        this.socket.addEventListener('close', () => {
            // try reconnect later
            setTimeout(() => this.connect(), 2000);
        });

        this.socket.addEventListener('error', () => {
            // close socket to trigger reconnect
            try { this.socket?.close(); } catch {}
        });
    }

    sendRequest(type: string, payload?: Record<string, JsonValue>): Promise<any> {
        return new Promise((resolve, reject) => {
            const sendNow = () => {
                if (!this.socket) {
                    reject(new Error('socket-not-open'));
                    return;
                }
                const doc: any = { type };
                if (payload) Object.assign(doc, payload);
                try {
                    this.socket.send(JSON.stringify(doc));
                    this.pending.push({resolve, reject});
                } catch (e) {
                    reject(e);
                }
            };

            if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
                this.connect();
                // wait for open
                const wait = () => {
                    if (!this.socket) return reject(new Error('socket-failed'));
                    if (this.socket.readyState === WebSocket.OPEN) return sendNow();
                    if (this.socket.readyState === WebSocket.CLOSED) return reject(new Error('socket-closed'));
                    setTimeout(wait, 50);
                };
                wait();
            } else {
                sendNow();
            }
        });
    }

    onMessage(cb: (msg:any)=>void) {
        this.listeners.push(cb);
        return () => {
            this.listeners = this.listeners.filter(x => x !== cb);
        };
    }
}

export const wsClient = new WSClient();

export default wsClient;
