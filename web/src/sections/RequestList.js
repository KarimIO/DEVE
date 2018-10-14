import React, { Component } from 'react';

export default class RequestList extends Component {
	render() {
		return (
			<section className="main-section request">
                <h2>Pending Requests</h2>
                <table>
                    <thead>
                        <tr>
                            <th></th>
                            <th>Views</th>
                            <th>Access</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                            <td></td>
                            <td><span>495</span></td>
                            <td><button>Access</button></td>
                        </tr>
                    </tbody>
                </table>
                <hr />
                <h2>Ongoing Requests</h2>
			</section>
		);
	}
}