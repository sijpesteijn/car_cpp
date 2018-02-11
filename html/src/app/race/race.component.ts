import {AfterViewInit, Component, OnDestroy} from "@angular/core";
import {Config} from "../app.config";
import {ObserverGroup, Race} from "./race.service";
import { RaceStripService } from "./race-strip.service";
import { Router } from "@angular/router";
import { Car, CarService } from "../car.service";

@Component({
    selector: 'race',
    template: require('./race.html'),
    styles: [require('./race.scss')]
})
export class RaceComponent {
    private raceWebsocket: WebSocket;
    private retry: any;
    private race: Race;
    private car: Car;
    private interval: any;

    constructor(private config: Config,
                private router: Router,
                private carService: CarService,
                private raceStripService: RaceStripService) {
    }

    ngOnInit() {
        // this.carService.getCar().subscribe(car => {
        //     this.car = car;
        //
        //     if (this.car.mode !== 2) {
        //         this.car.mode = 2;
        //         this.carService.saveCar(this.car).subscribe(() => this.setupWebsocket());
        //     }
        // });
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
            this.interval = setInterval(()=> {this.raceWebsocket.send('ping')}, 1000);
        };
        this.raceWebsocket.onmessage = (msg) => {
            if (msg.data) {
                const race = JSON.parse(msg.data);
                const org = this.raceStripService.stripRace(race);
                console.log('Race ', race.groups[0].observers[0]);
                if (!this.race) {
                    this.race = race;
                } else if (this.race && JSON.stringify(org) !== JSON.stringify(this.race)) {
                    this.race = race;
                }
            }
        };
        this.raceWebsocket.onerror   = (error) => {
            console.log('Error race websocket');
            clearInterval(this.interval);
            this.router.navigate(['./off']);
        };
        this.raceWebsocket.onclose   = (event) => {
            console.log('Closing race websocket');
            clearInterval(this.interval);
            this.router.navigate(['./off']);
        };
    }
}