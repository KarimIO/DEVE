import React, { Component } from 'react';

export class Input extends Component {
    render() {
      return (
        <div className="input-group">
            <input type={this.props.type} placeholder={this.props.placeholder} />
            <label>{this.props.label}</label>
        </div>);
    }
}

export class Password extends Component {
    constructor(props) {
        super(props);

        this.state = {
            show: false
        };
    }

    toggleShow = (e) => {
        e.preventDefault();

        this.setState({show: !this.state.show})
    }

    render() {
        let val = this.props.value;
        let ch = this.props.onChange;

        return (
        <div className="input-group password">
            <input type={this.state.show ? "text" : "password"} placeholder={this.props.placeholder} value={val ? val : undefined} onChange={ch ? ch : null} />
            <label>{this.props.label}</label>
            <div className={`eye${this.state.show ? "" : " hide"}`} onClick={this.toggleShow}>Show</div>
        </div>);
    }
}