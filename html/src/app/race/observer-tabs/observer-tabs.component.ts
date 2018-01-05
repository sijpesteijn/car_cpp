import {Component, Input, OnDestroy} from '@angular/core';
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
    @Input()
    set race(race: Race) {
        if (race) {
            if (race.name !== this.selectedRace.name) {
                this.selectedRace = race;
            } else {
                for (let i = 0; i < this.selectedRace.groups.length; i++) {
                    const orgGrp = JSON.parse(JSON.stringify(this.selectedRace.groups[i]));
                    orgGrp.observers.forEach(observer => {
                        delete observer.roi.type;
                        delete observer.roi.color;
                    });
                    const newGrp = race.groups[i];
                    if (!newGrp) {
                        delete this.selectedRace.groups[i];
                    } else if (JSON.stringify(orgGrp) !== JSON.stringify(newGrp)) {
                        // console.log('Org ', JSON.stringify(orgGrp));
                        // console.log('New ', JSON.stringify(newGrp));
                        this.selectedRace.groups[i] = newGrp;
                    }
                }
            }
        }
    }

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