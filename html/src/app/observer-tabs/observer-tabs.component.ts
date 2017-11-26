import {Component, EventEmitter} from '@angular/core';
import {Race, RaceService} from "../race.service";
import {CarObserver} from "../observers/observer.service";
import {Observable} from "rxjs/Observable";
@Component({
    selector: 'observer-tabs',
    template: require('./observer-tabs.html'),
    styles: [require('./observer-tabs.scss')]
})
export class ObserverTabsComponent {
    private started = false;
    private races: Race[];
    private selectedRace: Race;

    constructor(private raceService: RaceService) {}

    ngOnInit() {
        this.raceService.getRaces().subscribe(races => {
            this.races = races;
            if (this.races.length > 0) {
                this.loadRace(this.races[0]);
            }
        });
        this.raceService.getRace().subscribe(race => this.selectedRace = race);
    }

    loadRace(race: Race) {
        this.raceService.loadRace(race.name).subscribe(race => {
            this.selectedRace = race;
        });
    }

    startRace() {
        this.selectedRace.running = 1;
        this.raceService.saveRace(this.selectedRace).subscribe(() => {
            this.started = true;
        });
    }

    stopRace() {
        this.selectedRace.running = 0;
        this.raceService.saveRace(this.selectedRace).subscribe(() => {
            this.started = false;
        });
    }

    updateObserver(observer: CarObserver) {
        const updateObs = this.selectedRace.observers.filter(obs => obs.type === observer.type);
        if (updateObs.length > 0) {
            updateObs[0] = observer;
            this.raceService.saveRace(this.selectedRace).subscribe(() => {});
        }
    }
}