import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";

export interface ObserverState {
    type: string;
    active: boolean;
}

@Injectable()
export class ObserversStateMessagesService {
    private logWebsocket: WebSocket;
    private retry: any;
    private log: Subject<any> = new BehaviorSubject(null);

    constructor(private config: Config) {
        this.startReconnect();
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.logWebsocket = new WebSocket(this.config.get('observer.ws'));
                clearInterval(this.retry);
                this.logWebsocket.onopen    = (evt) => {
                };
                this.logWebsocket.onmessage = (msg) => {
                    if (msg.data) {
                        this.log.next(JSON.parse(msg.data));
                    }
                };
                this.logWebsocket.onerror   = (error) => {
                    console.log(error);
                    // this.startReconnect();
                };
                this.logWebsocket.onclose   = (event) => {
                    // this.startReconnect();
                };
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }

    getLog(): Observable<ObserverState[]> {
        return this.log;
    }
}