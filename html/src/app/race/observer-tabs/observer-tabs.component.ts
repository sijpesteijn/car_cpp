import {Component, Input, OnDestroy} from '@angular/core';
import {Race, CarObserver, RaceService, ObserverGroup} from '../race.service';
import { RaceStripService } from "../race-strip.service";
import { EventService, OBSERVER_METADATA_CHANGED } from "../../event.service";
@Component({
    selector: 'observer-tabs',
    template: require('./observer-tabs.html'),
    styles: [require('./observer-tabs.scss')]
})
export class ObserverTabsComponent implements OnDestroy {
    private started = false;
    private races: Race[];
    private selectedRace: Race;
    @Input()
    set race(race: Race) {
        if (race) {
            if (race.name !== this.selectedRace.name) {
                this.selectedRace = race;
            } else {
                for (let i = 0; i < this.selectedRace.groups.length; i++) {
                    const newGrp = race.groups[i];
                    if (!newGrp) {
                        delete this.selectedRace.groups[i];
                    } else {
                        const orgGrp = this.selectedRace.groups[i];
                        for (let j = 0; j < orgGrp.observers.length; j++) {
                            const currObs = JSON.stringify(this.raceStripService.stripObserver(this.raceStripService.clone(orgGrp.observers[j])));
                            const newObs = JSON.stringify(this.raceStripService.stripObserver(this.raceStripService.clone(race.groups[i].observers[j])));
                            if (currObs !== newObs) {
                                orgGrp.observers[j] = race.groups[i].observers[j];
                            } else {
                                this.eventService.emit(OBSERVER_METADATA_CHANGED, race.groups[i].observers[j]);
                            }
                        }
                    }
                }
            }
        }
    }

    constructor(private raceService: RaceService, private raceStripService: RaceStripService, private eventService: EventService) {}

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
        if(this.selectedRace && this.selectedRace.running) {
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
        this.raceService.saveRace().subscribe(() => {
            this.started = true;
        });
    }

    stopObserversGroups(groups: ObserverGroup[]) {
        groups.forEach(group => group.observers.forEach(observer => {
            observer.selected = false;
            observer.condition_achieved = false;
        }));
    }

    stopRace() {
        if (this.selectedRace) {
            this.selectedRace.running = false;
            this.stopObserversGroups(this.selectedRace.groups);
            this.raceService.saveRace().subscribe(() => {
                this.started = false;
            });
        }
    }

    updateRace(newGroup: ObserverGroup) {
         this.raceService.saveRace().subscribe(() => {});
    }
}