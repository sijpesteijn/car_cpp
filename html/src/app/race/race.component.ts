import {AfterViewInit, Component, OnDestroy} from "@angular/core";
import {Config} from "../app.config";
import {ObserverGroup, Race} from "./race.service";
import { RaceStripService } from "./race-strip.service";

@Component({
    selector: 'race',
    template: require('./race.html'),
    styles: [require('./race.scss')]
})
export class RaceComponent implements AfterViewInit, OnDestroy {
    private raceWebsocket: WebSocket;
    private retry: any;
    private race: Race;

    constructor(private config: Config, private raceStripService: RaceStripService) {
    }

    ngAfterViewInit() {
        setTimeout( () => {
            this.setupWebsocket();
        }, 500)
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
                const org = this.raceStripService.stripRace(race);
                if (!this.race) {
                    this.race = race;
                } else if (this.race && JSON.stringify(org) !== JSON.stringify(this.race)) {
                    this.race = race;
                }
            }
        };
        this.raceWebsocket.onerror   = (error) => {
            console.log('Error race websocket');
        };
        this.raceWebsocket.onclose   = (event) => {
            console.log('Closing race websocket');
        };
    }
}