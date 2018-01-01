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
        this.setupWebsocket();
    }

    ngOnDestroy() {
        clearInterval(this.retry);
        if (this.raceWebsocket) {
            this.raceWebsocket.close();
        }
    }

    private setupWebsocket() {
        this.raceWebsocket = new WebSocket(this.config.get('race.status'));
        this.raceWebsocket.onopen    = () => {
            console.log('Opening race websocket');
        };
        this.raceWebsocket.onmessage = (msg) => {
            if (msg.data) {
                const race = JSON.parse(msg.data);
                const org = this.cloneAndStrip(race);
                // console.log('Org race ', this.race);
                // console.log('New race ', race);
                if (!this.race) {
                    this.race = race;
                } else if (this.race && JSON.stringify(org) !== JSON.stringify(this.race)) {
                    this.race = race;
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
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.setupWebsocket();
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }

    private cloneAndStrip(race: Race) {
        const org: Race = JSON.parse(JSON.stringify(race));
        this.stripObserversGroups(org.groups);
        return org;
    }

    private stripObserversGroups(groups: ObserverGroup[]) {
        groups.forEach(group => group.observers.forEach(observer => {
            delete observer.roi.type;
            delete observer.roi.color;
        }));
    }
}