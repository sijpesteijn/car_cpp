import {AfterViewInit, Component, OnDestroy} from "@angular/core";
import {Config} from "../app.config";
import {ObserverGroup, Race} from "./race.service";

@Component({
    selector: 'race',
    template: require('./race.html'),
    styles: [require('./race.scss')]
})
export class RaceComponent implements AfterViewInit, OnDestroy {
    private raceWebsocket: WebSocket;
    private retry: any;
    private race: Race;

    constructor(private config: Config) {
    }

    ngAfterViewInit() {
        this.startReconnect();
    }

    ngOnDestroy() {
        clearInterval(this.retry);
        if (this.raceWebsocket) {
            this.raceWebsocket.close();
        }
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.raceWebsocket = new WebSocket(this.config.get('race.status'));
                clearInterval(this.retry);
                this.raceWebsocket.onopen    = () => {
                    console.log('Opening race websocket');
                };
                this.raceWebsocket.onmessage = (msg) => {
                    if (msg.data) {
                        const race = JSON.parse(msg.data);
                        const org = this.cloneAndStrip(race);
                        if (JSON.stringify(org) !== JSON.stringify(race)) {
                            console.log('Race');
                            this.race.next(race);
                        }
                    }
                };
                this.raceWebsocket.onerror   = (error) => {
                    console.log('Error race websocket');
                    // this.startReconnect();
                };
                this.raceWebsocket.onclose   = (event) => {
                    console.log('Closing race websocket');
                    // this.startReconnect();
                };
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }

    private cloneAndStrip(race: Race) {
        const org: Race = JSON.parse(JSON.stringify(race));
        this.stripObserversGroup(org.group);
        return org;
    }

    private stripObserversGroup(group: ObserverGroup) {
        group.observers.forEach(observer => {
            delete observer.roi.type;
            delete observer.roi.color;
        });
        if (group.group) {
            this.stripObserversGroup(group.group);
        }
    }
}