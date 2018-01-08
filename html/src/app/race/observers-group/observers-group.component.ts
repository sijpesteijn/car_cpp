import {Component, EventEmitter, Input, Output} from "@angular/core";
import { CarObserver, ObserverGroup } from "../race.service";
import { RaceStripService } from "../race-strip.service";

@Component({
    selector: 'observers-group',
    template: require('./observers-group.html')
})
export class ObserversGroupComponent {
    @Input()
    set group(group: ObserverGroup) {
        this._group = group;
    };
    private _group: ObserverGroup;
    @Input() running: boolean;
    @Output() onChange = new EventEmitter();

    constructor(private raceStripService: RaceStripService) {}

    private updateObserver($event: CarObserver) {
        if ($event) {
            let index = this._group.observers.findIndex(obs => obs.type === $event.type);
            const strippedOld = JSON.stringify(this.raceStripService
                .stripObserver(this.raceStripService.clone(this._group.observers[index])));
            const strippedNew = JSON.stringify(this.raceStripService.stripObserver(this.raceStripService.clone($event)));
            if (strippedNew !== strippedOld) {
                this._group.observers[index]= $event;
                this.onChange.emit(this._group);
            }
        }
    }
}