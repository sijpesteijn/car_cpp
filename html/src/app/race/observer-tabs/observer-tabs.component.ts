import {Component, OnDestroy} from '@angular/core';
import {Race, CarObserver, RaceService, ObserverGroup} from '../race.service';
@Component({
    selector: 'observer-tabs',
    template: require('./observer-tabs.html'),
    styles: [require('./observer-tabs.scss')]
})
export class ObserverTabsComponent implements OnDestroy {
    private started = false;
    private races: Race[];
    private selectedRace: Race;

    constructor(private raceService: RaceService) {}

    ngOnInit() {
        this.raceService.getRaces().subscribe(races => {
            this.races = races;
            if (this.races.length > 0) {
                this.selectRace(this.races[0]);
            }
        });
        this.raceService.getRace().subscribe(race => {
            if (race) {
                this.selectedRace = race;
            }
        });
    }

    ngOnDestroy() {
        if(this.selectedRace.running) {
            this.stopRace();
        }
    }

    selectRace(race: Race) {
        this.raceService.selectRace(race.name).subscribe(race => {
            this.selectedRace = race;
            this.stopRace();
        });
    }

    startRace() {
        this.selectedRace.running = true;
        this.raceService.saveRace(this.selectedRace).subscribe(() => {
            this.started = true;
        });
    }

    stopObserversGroup(group: ObserverGroup) {
        group.observers.forEach(observer => {
            observer.active = false;
            observer.condition_achieved = false;
        });
        if (group.group) {
            this.stopObserversGroup(group.group);
        }
    }

    stopRace() {
        this.selectedRace.running = false;
        this.stopObserversGroup(this.selectedRace.group);
        this.raceService.saveRace(this.selectedRace).subscribe(() => {
            this.started = false;
        });
    }

    private replaceObserver(group: ObserverGroup, observer: CarObserver) {
        const updateObs = group.observers.filter(obs => obs.type === observer.type);
        if (updateObs.length > 0) {
            updateObs[0] = observer;
        } else if(group.group) {
            this.replaceObserver(group.group, observer);
        }
    }

    updateObserver(observer: CarObserver) {
        this.replaceObserver(this.selectedRace.group, observer);
        this.raceService.saveRace(this.selectedRace).subscribe(() => {});
    }
}